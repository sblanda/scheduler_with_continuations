#ifndef CS_BASE_H
#define CS_BASE_H

class CSBase {
public:
    virtual ~CSBase() = default;
    virtual void schedule_continuation() = 0;
};

#endif