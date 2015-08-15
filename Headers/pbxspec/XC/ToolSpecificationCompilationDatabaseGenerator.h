// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_XC_ToolSpecificationCompilationDatabaseGenerator_h
#define __pbxspec_XC_ToolSpecificationCompilationDatabaseGenerator_h

#include <pbxspec/PBX/Tool.h>

namespace pbxspec { namespace XC {

class ToolSpecificationCompilationDatabaseGenerator : public PBX::Tool {
protected:
    friend class Tool;
    ToolSpecificationCompilationDatabaseGenerator(bool isDefault);

public:
    inline std::string const &isa() const
    { return PBX::Tool::isa(); }
    bool isa(std::string const &isa) const override
    { return (PBX::Tool::isa(isa) ||
              isa == this->isa() ||
              isa == PBX::Tool::Isa()); }

public:
    static inline char const *Isa()
    { return ISA::XCToolSpecificationCompilationDatabaseGenerator; }
};

} }

#endif  // !__pbxspec_XC_ToolSpecificationCompilationDatabaseGenerator_h
