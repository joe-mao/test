#ifndef GLOBAL_H
#define GLOBAL_H

class Global
{

public:
    static Global * getInstance();

private:
    Global();
public:
    static Global * instance;

};

#endif // GLOBAL_H
