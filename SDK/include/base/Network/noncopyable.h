#ifndef __QZ_COMMON_NONCOPYABLE_H__
#define __QZ_COMMON_NONCOPYABLE_H__

class noncopyable
{
protected:
    noncopyable (void)
    {

    }

    ~noncopyable (void)
    {

    }

private:  // emphasize the following members are private
    noncopyable (const noncopyable&);
    const noncopyable& operator= (const noncopyable&);
};

#endif //__TTC_NONCOPYABLE_H__


