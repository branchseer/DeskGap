#include "control_geometry.h"
#include <cmath>

namespace DeskGap {
    using std::array;
    using System::Windows::Forms::Control;
    using std::round;
    using System::Drawing::SizeF;
    array<int, 2> LogicalToDevice(Control^ control, const array<int, 2>& sizeOrPoint) {
        SizeF formScaleDimensions = control->FindForm()->CurrentAutoScaleDimensions;
        return {
            int(float(sizeOrPoint[0]) * formScaleDimensions.Width / 6),
            int(float(sizeOrPoint[1]) * formScaleDimensions.Height / 13),
        };
    }
    array<int, 2> DeviceToLogical(Control^ control, const array<int, 2>& sizeOrPoint) {
        SizeF formScaleDimensions = control->FindForm()->CurrentAutoScaleDimensions;
        return {
            int(round(float(sizeOrPoint[0]) / formScaleDimensions.Width * 6)),
            int(round(float(sizeOrPoint[1]) / formScaleDimensions.Height * 13)),
        };
    }
}
