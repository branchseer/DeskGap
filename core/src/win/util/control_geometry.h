#ifndef util_control_geometry_h
#define util_control_geometry_h

#include <array>
namespace DeskGap {
    std::array<int, 2> LogicalToDevice(System::Windows::Forms::Control^, const std::array<int, 2>&);
    std::array<int, 2> DeviceToLogical(System::Windows::Forms::Control^, const std::array<int, 2>&);
}

#endif
