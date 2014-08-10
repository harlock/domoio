#ifndef DATABASE_H
#define DATABASE_H
#include <libpq-fe.h>

namespace domoio {
  namespace db {
    void connect(void);
    void close(void);

    PGresult *exec(const char*);
  }
}


#endif //DATABASE_H