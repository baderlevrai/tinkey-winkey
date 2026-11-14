#include "tinky.h"

int main(int ac, char **av) {
    if (ac > 1) { // launched by user
        return (manager_dispatch(av));
    } else { // launched by SCM
        SERVICE_TABLE_ENTRY serviceTable[] =
        {
            {"tinky", ServiceMain},
            {NULL, NULL}
        };
        if (!StartServiceCtrlDispatcher(serviceTable)) 
            return (1);
    }
    return (0);
}
