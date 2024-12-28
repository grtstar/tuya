#include "tkl_init_pm.h"



#define TKL_PM_DESC_DEF(__type, __name, ...)           \
    CONST __type  c_##__name = {                    \
        __VA_ARGS__                                 \
    };                                              \
    __type* tkl_##__name##_get(VOID_T) {            \
        return (__type*)&c_##__name;                \
    }

#define TKL_PM_DESC_INIT(__key)             .__key = NULL,

#define PM_DESC_ITEM                   TKL_DESC_INIT

#if defined(CONFIG_ENABLE_PM) && (CONFIG_ENABLE_PM == 1)
#undef  PM_DESC_ITEM
#define PM_DESC_ITEM(__key)           .__key = tkl_pm_##__key,
#endif


// TKL_PM_DESC_DEF(
//     TKL_PM_DESC_T,
//     pm_desc,
//     PM_DESC_ITEM(dev_unregistor)
//     PM_DESC_ITEM(get_dev_info)
//     PM_DESC_ITEM(get_dev_list_head)
//     PM_DESC_ITEM(set_voltage)
//     PM_DESC_ITEM(get_voltage)
//     PM_DESC_ITEM(set_current)
//     PM_DESC_ITEM(get_current)
//     PM_DESC_ITEM(enable)
//     PM_DESC_ITEM(disable)
//     PM_DESC_ITEM(is_enable)
//     PM_DESC_ITEM(power_off)
//     PM_DESC_ITEM(reset)
//     PM_DESC_ITEM(ioctl)
// )

TKL_PM_INTF_T c_pm_desc = {
    .dev_unregistor = tkl_pm_dev_unregistor,
    .get_dev_info   = tkl_pm_get_dev_info,
    .get_dev_list_head = tkl_pm_get_dev_list_head,
    .set_voltage   = tkl_pm_set_voltage,
    .get_voltage = tkl_pm_get_voltage,
    .set_current   = tkl_pm_set_current,
    .get_current = tkl_pm_get_current,
    .enable   = tkl_pm_enable,
    .disable = tkl_pm_disable,
    .is_enable   = tkl_pm_is_enable,
    .power_off = tkl_pm_power_off,
    .reset   = tkl_pm_reset,
    .ioctl   = tkl_pm_ioctl
};
// CONST TKL_PM_DESC_T c_pm_desc={
//     .dev_unregistor =tkl_pm_dev_unregistor;
// };

TKL_PM_INTF_T* tkl_pm_desc_get()
{
    return &c_pm_desc;
}