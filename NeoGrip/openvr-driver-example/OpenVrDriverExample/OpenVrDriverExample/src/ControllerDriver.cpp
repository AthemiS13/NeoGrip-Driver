#include <ControllerDriver.h>

EVRInitError ControllerDriver::Activate(uint32_t unObjectId)
{
    driverId = unObjectId; // Unique ID for your driver

    PropertyContainerHandle_t props = VRProperties()->TrackedDeviceToPropertyContainer(driverId); // Container for driver properties

    VRProperties()->SetStringProperty(props, Prop_InputProfilePath_String, "{example}/input/controller_profile.json"); // Input profile path
    VRProperties()->SetInt32Property(props, Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_LeftHand); // Device role

    // Create scalar components for joystick
    VRDriverInput()->CreateScalarComponent(props, "/input/joystick/y", &joystickYHandle, EVRScalarType::VRScalarType_Absolute,
        EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
    VRDriverInput()->CreateScalarComponent(props, "/input/joystick/x", &joystickXHandle, EVRScalarType::VRScalarType_Absolute,
        EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);

    // Create button components for buttons and grip/trigger
    VRDriverInput()->CreateBooleanComponent(props, "/input/x/click", &buttonXHandle);
    VRDriverInput()->CreateBooleanComponent(props, "/input/y/click", &buttonYHandle);
    VRDriverInput()->CreateBooleanComponent(props, "/input/grip/value", &buttonGrabHandle);
    VRDriverInput()->CreateBooleanComponent(props, "/input/trigger/value", &buttonTriggerHandle);
    VRDriverInput()->CreateBooleanComponent(props, "/input/joystick/click", &joystickClickHandle);
    VRDriverInput()->CreateBooleanComponent(props, "/input/system/click", &systemClickHandle);

    return VRInitError_None;
}

DriverPose_t ControllerDriver::GetPose()
{
    DriverPose_t pose = { 0 }; // Default pose
    pose.poseIsValid = false;
    pose.result = TrackingResult_Calibrating_OutOfRange;
    pose.deviceIsConnected = true;

    HmdQuaternion_t quat;
    quat.w = 1;
    quat.x = 0;
    quat.y = 0;
    quat.z = 0;

    pose.qWorldFromDriverRotation = quat;
    pose.qDriverFromHeadRotation = quat;

    return pose;
}

void ControllerDriver::RunFrame()
{
    static bool buttonXState = false;
    static bool buttonYState = false;
    static bool triggerState = false;

    // Update joystick values (e.g., move forward)
    VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.95f, 0); // Forward
    VRDriverInput()->UpdateScalarComponent(joystickXHandle, 0.0f, 0); // No sideways movement

    // Toggle button states for testing
    buttonXState = !buttonXState; // Toggle state for Button X
    buttonYState = !buttonYState; // Toggle state for Button Y
    triggerState = !triggerState; // Toggle state for Trigger

    // Update button states (example states)
    VRDriverInput()->UpdateBooleanComponent(buttonXHandle, buttonXState, 0);   // Button X toggled
    VRDriverInput()->UpdateBooleanComponent(buttonYHandle, buttonYState, 0);   // Button Y toggled
    VRDriverInput()->UpdateBooleanComponent(buttonGrabHandle, false, 0);       // Grip not pressed
    VRDriverInput()->UpdateBooleanComponent(buttonTriggerHandle, triggerState, 0); // Trigger toggled
    VRDriverInput()->UpdateBooleanComponent(joystickClickHandle, false, 0); // Joystick click not pressed
    VRDriverInput()->UpdateBooleanComponent(systemClickHandle, false, 0);   // System click not pressed
}

void ControllerDriver::Deactivate()
{
    driverId = k_unTrackedDeviceIndexInvalid;
}

void* ControllerDriver::GetComponent(const char* pchComponentNameAndVersion)
{
    if (strcmp(IVRDriverInput_Version, pchComponentNameAndVersion) == 0)
    {
        return this;
    }
    return NULL;
}

void ControllerDriver::EnterStandby() {}

void ControllerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1)
    {
        pchResponseBuffer[0] = 0;
    }
}
