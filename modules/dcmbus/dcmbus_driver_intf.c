#include "dcmbus_driver_intf.h"

extern struct dcmbus_driver_ops dcmbus_driver_ethernet_tcp_ops;
extern struct dcmbus_driver_ops dcmbus_driver_ethernet_udp_ops;
extern struct dcmbus_driver_ops dcmbus_driver_non_blocking_tcp_ops;
struct dcmbus_driver_ops *dcmbus_drivers[] = {
    &dcmbus_driver_ethernet_tcp_ops, &dcmbus_driver_ethernet_udp_ops,
    &dcmbus_driver_non_blocking_tcp_ops, NULL
};
