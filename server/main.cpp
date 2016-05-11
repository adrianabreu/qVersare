#include <QCoreApplication>
#include <QFile>

#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> //fork, clase
#include <pwd.h> //getpwnam
#include <fcntl.h> //open, creat
#include <grp.h>

#include "qversaredaemon.h"
#include "serversettings.h"
#include "utils.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("ULL");
    QCoreApplication::setOrganizationDomain("ull.es");
    QCoreApplication::setApplicationName("qVersare");

    ServerSettings* mySettings = new ServerSettings(&a);
    bool daemonMode = mySettings->getDaemon();
    if(daemonMode) {
        pid_t pid;

        // Nos clonamos a nosotros mismos creando un proceso hijo
        pid = fork();

        // Si pid es < 0, fork() falló
        if (pid < 0) {
            // Mostrar la descripción del error y terminar
            syslog(LOG_NOTICE,"error fork pid");
            exit(10);
        }

        // Si pid es > 0, estamos en el proceso padre
        if (pid > 0) {
            // Terminar el proceso
            exit(0);
        }

        // Si la ejecución llega a este punto, estamos en el hijo
        umask(0);
        // Abrir una conexión al demonio syslog
        openlog(argv[0], LOG_NOWAIT | LOG_PID, LOG_USER);
        // Cerrar los descriptores de la E/S estándar
        close(STDIN_FILENO);                    // fd 0
        close(STDOUT_FILENO);                   // fd 1
        close(STDERR_FILENO);                   // fd 2

        int fd0 = open("/dev/null", O_RDONLY);  // fd0 == 0
        int fd1 = open("/dev/null", O_WRONLY);  // fd0 == 1
        int fd2 = open("/dev/null", O_WRONLY);  // fd0 == 2

        pid_t sid;

        // Intentar crear una nueva sesión
        sid = setsid();
        if (sid < 0) {
            syslog(LOG_ERR, "No fue posible crear una nueva sesión\n");
            exit(11);
        }

        //Antes de cambiar los permisos, vamos a guardar nuestro pid para
        //el script
        QFile file("/var/run/qVersareServer.pid");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            //out << sid;
            out << getpid();
            file.close();
        } else {
            syslog(LOG_ERR, "No se pudo guardar el pid en /var/run, root?");
            exit(17);
        }

        // Cambiar el usuario efectivo del proceso a 'midemonio'
        passwd* user = getpwnam("qversaredaemon");
        if (user != NULL) {
            seteuid(user->pw_uid);
        } else {
            syslog(LOG_ERR, "No existe el user qversaredaemon");
            exit(12);
        }
        // Cambiar el grupo efectivo del proceso a 'midemonio'
        group* daemonGroup = getgrnam("qversaredaemon");
        if (daemonGroup != NULL) {
            setegid(daemonGroup->gr_gid);
        } else {
            syslog(LOG_ERR, "No existe el grupo qversaredaemon");
        }
    }

    QVersareDaemon myDaemon(mySettings, &a);
    myDaemon.setupUnixSignalHandlers();
    int tempReturn = a.exec();
    //Si estabamos en demonio, cierra el log muchacho y manda una despedida
    if(daemonMode) {
        helperDebug(true, "Finalizada la ejecucion con exito");
        closelog();
    }
    return tempReturn;
}
