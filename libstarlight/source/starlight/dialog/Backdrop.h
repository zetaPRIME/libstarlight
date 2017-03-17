#pragma once
#include "starlight/_global.h"

#include <string>

#include "starlight/ThemeManager.h"

#include "starlight/ui/Form.h"

namespace starlight {
    namespace dialog {
        class Backdrop : public ui::Form, public ui::FormCreator<Backdrop> {
        private:
            //
        
        public:
            Backdrop(std::string imgPath = "decorations/generic backdrop");
            //~Backdrop() override { };
        };
    }
}
