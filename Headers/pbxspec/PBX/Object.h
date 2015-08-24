// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_PBX_Object_h
#define __pbxspec_PBX_Object_h

#include <pbxspec/Base.h>
#include <pbxspec/ISA.h>
#include <pbxspec/Types.h>

namespace pbxspec { namespace PBX {

class Object {
public:
    typedef std::shared_ptr <Object> shared_ptr;
    typedef std::vector <shared_ptr> vector;
    typedef std::map <std::string, shared_ptr> map;

private:
    std::string _isa;

protected:
    Object(std::string const &isa);

public:
    inline std::string const &isa() const
    { return _isa; }

public:
    virtual inline bool isa(std::string const &isa) const
    { return (_isa == isa); }

public:
    template <typename T>
    inline bool isa() const
    { return (T::Isa() == isa()); }
};

} }

#endif  // !__pbxspec_PBX_Object_h
