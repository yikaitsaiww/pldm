#pragma once

#include "libpldm/pldm.h"

#include "common/instance_id.hpp"
#include "common/types.hpp"
#include "oem_event_manager.hpp"
#include "platform-mc/manager.hpp"
#include "requester/handler.hpp"
#include "requester/request.hpp"

namespace pldm
{
namespace oem_ampere
{
using namespace pldm::pdr;

using EventToMsgMap_t = std::unordered_map<uint8_t, std::string>;

enum sensor_ids
{
    BOOT_OVERALL = 175,
};

namespace boot
{
namespace status
{
enum boot_status
{
    BOOT_STATUS_SUCCESS = 0x80,
    BOOT_STATUS_FAILURE = 0x81,
};
} // namespace status
namespace stage
{
enum boot_stage
{
    UEFI_STATUS_CLASS_CODE_MIN = 0x00,
    UEFI_STATUS_CLASS_CODE_MAX = 0x7f,
    SECPRO = 0x90,
    MPRO = 0x91,
    ATF_BL1 = 0x92,
    ATF_BL2 = 0x93,
    DDR_INITIALIZATION = 0x94,
    DDR_TRAINING = 0x95,
    S0_DDR_TRAINING_FAILURE = 0x96,
    ATF_BL31 = 0x97,
    ATF_BL32 = 0x98,
    S1_DDR_TRAINING_FAILURE = 0x99,
};
} // namespace stage
} // namespace boot

enum class log_level : int
{
    OK,
    BIOSFWPANIC,
};

/**
 * @brief OemEventManager
 *
 *
 */
class OemEventManager
{
  public:
    OemEventManager() = delete;
    OemEventManager(const OemEventManager&) = delete;
    OemEventManager(OemEventManager&&) = delete;
    OemEventManager& operator=(const OemEventManager&) = delete;
    OemEventManager& operator=(OemEventManager&&) = delete;
    virtual ~OemEventManager() = default;

    explicit OemEventManager(
        sdeventplus::Event& event,
        requester::Handler<requester::Request>* /* handler */,
        pldm::InstanceIdDb& /* instanceIdDb */) : event(event) {};

    /** @brief Decode sensor event messages and handle correspondingly.
     *
     *  @param[in] request - the request message of sensor event
     *  @param[in] payloadLength - the payload length of sensor event
     *  @param[in] formatVersion - the format version of sensor event
     *  @param[in] tid - TID
     *  @param[in] eventDataOffset - the event data offset of sensor event
     *
     *  @return int - returned error code
     */
    int handleSensorEvent(const pldm_msg* request, size_t payloadLength,
                          uint8_t /* formatVersion */, pldm_tid_t tid,
                          size_t eventDataOffset);

  protected:
    /** @brief Create prefix string for logging message.
     *
     *  @param[in] tid - TID
     *  @param[in] sensorId - Sensor ID
     *
     *  @return std::string - the prefeix string
     */
    std::string prefixMsgStrCreation(pldm_tid_t tid, uint16_t sensorId);

    /** @brief Log the message into Redfish SEL.
     *
     *  @param[in] description - the logging message
     *  @param[in] logLevel - the logging level
     */
    void sendJournalRedfish(const std::string& description,
                            log_level& logLevel);

    /** @brief Convert the one-hot DIMM index byte into a string of DIMM
     * indexes.
     *
     *  @param[in] dimmIdxs - the one-hot DIMM index byte
     *
     *  @return std::string - the string of DIMM indexes
     */
    std::string dimmIdxsToString(uint32_t dimmIdxs);

    /** @brief Handle numeric sensor event message from boot overall sensor.
     *
     *  @param[in] tid - TID
     *  @param[in] sensorId - Sensor ID
     *  @param[in] presentReading - the present reading of the sensor
     */
    void handleBootOverallEvent(pldm_tid_t /*tid*/, uint16_t /*sensorId*/,
                                uint32_t presentReading);

    /** @brief Handle numeric sensor event messages.
     *
     *  @param[in] tid - TID
     *  @param[in] sensorId - Sensor ID
     *  @param[in] sensorData - the sensor data
     *  @param[in] sensorDataLength - the length of sensor data
     *
     *  @return int - returned error code
     */
    int processNumericSensorEvent(pldm_tid_t tid, uint16_t sensorId,
                                  const uint8_t* sensorData,
                                  size_t sensorDataLength);

    /** @brief Handle state sensor event messages.
     *
     *  @param[in] tid - TID
     *  @param[in] sensorId - Sensor ID
     *  @param[in] sensorData - the sensor data
     *  @param[in] sensorDataLength - the length of sensor data
     *
     *  @return int - returned error code
     */
    int processStateSensorEvent(pldm_tid_t tid, uint16_t sensorId,
                                const uint8_t* sensorData,
                                size_t sensorDataLength);

    /** @brief Handle op state sensor event messages.
     *
     *  @param[in] tid - TID
     *  @param[in] sensorId - Sensor ID
     *  @param[in] sensorData - the sensor data
     *  @param[in] sensorDataLength - the length of sensor data
     *
     *  @return int - returned error code
     */
    int processSensorOpStateEvent(pldm_tid_t tid, uint16_t sensorId,
                                  const uint8_t* sensorData,
                                  size_t sensorDataLength);

    /** @brief reference of main event loop of pldmd, primarily used to schedule
     *  work
     */
    sdeventplus::Event& event;
};
} // namespace oem_ampere
} // namespace pldm
