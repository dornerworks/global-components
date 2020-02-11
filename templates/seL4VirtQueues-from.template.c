/*
 * Copyright 2018, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

/*- from 'global-endpoint.template.c' import allocate_cap with context -*/

#include <sel4/sel4.h>
#include <camkes/dataport.h>
#include <camkes.h>
#include <camkes/virtqueue_template.h>
#include <virtqueue.h>

/*- if len(me.parent.to_ends) != 1 -*/
    /*? raise(Exception('%s must only have 1 to end' % (me.parent.name))) ?*/
/*- endif -*/
/*- set to_end = me.parent.to_ends[0] -*/

/*- set all_connection_ends = me.parent.from_ends -*/

/*- set interface_name =  me.interface.type.name -*/

/*- if interface_name == "VirtQueueDrv" -*/
    /*- set end_string = "drv" -*/
    /*- set other_end_string = "dev" -*/
/*- else -*/
    /*- set end_string = "dev" -*/
    /*- set other_end_string = "drv" -*/
/*- endif -*/

/*- set topology = configuration[to_end.instance.name].get("%s_topology" % to_end.interface.name, []) -*/
/*- set topology_entry = [] -*/

/*- for entry in topology -*/
    /*- if entry[end_string] == "%s.%s" % (me.instance.name, me.interface.name) -*/
        /*- do topology_entry.append(entry) -*/
    /*- endif -*/
/*- endfor -*/

/*- if len(topology_entry) != 1 -*/
    /*? raise(Exception('Could not find topology entry for: %s.%s' % (me.instance.name, me.interface.name))) ?*/
/*- endif -*/

/*# Check that there is a valid interface on the other end of the topology from us #*/
/*- set other_interface_name = topology_entry[0][other_end_string] -*/
/*- set other_interface = [0] -*/
/*- for c in all_connection_ends -*/
  /*- if str(c) == other_interface_name -*/
    /*- do other_interface.__setitem__(0, c) -*/
  /*- endif -*/
/*- endfor -*/
/*- if other_interface[0] == 0 -*/
    /*? raise(TemplateError('Interface %s is not present in connection.' % (other_interface_name))) ?*/
/*- endif -*/
/*- set other_interface = other_interface[0] -*/

/*# Create shared memory region between the two interfaces #*/
/*- set shmem_size = configuration[me.instance.name].get("%s_shmem_size" % me.interface.name, 4096) -*/
/*- if shmem_size != configuration[other_interface.instance.name].get("%s_shmem_size" % other_interface.interface.name, 4096) -*/
    /*? raise(TemplateError('Setting %s.%s_shmem_size does not match size configuration from other side: %d vs. %d' % (me.instance.name, me.interface.name, shmem_size, configuration[other_interface.instance.name].get("%s_shmem_size" % other_interface.interface.name, 4096)))) ?*/
/*- endif -*/
/*- if end_string == 'drv' -*/
    /*- set shmem_symbol = '%s_%s_data' % (str(me), other_interface_name) -*/
/*- else -*/
    /*- set shmem_symbol = '%s_%s_data' % (other_interface_name, str(me)) -*/
/*- endif -*/
/*- set shmem_symbol = shmem_symbol.replace('.', '_') -*/
/*- set page_size = macros.get_page_size(shmem_size, options.architecture) -*/
/*- if page_size == 0 -*/
  /*? raise(TemplateError('Setting %s.%s_shmem_size does not meet minimum size and alignment requirements. %d must be at least %d and %d aligned' % (me.instance.name, me.interface.name, size, 4096, 4096))) ?*/
/*- endif -*/

/*? macros.shared_buffer_symbol(sym=shmem_symbol, shmem_size=shmem_size, page_size=page_size) ?*/
/*? register_shared_variable(shmem_symbol, shmem_symbol, shmem_size, frame_size=page_size) ?*/

size_t /*? me.interface.name ?*/_get_size(void) {
    return sizeof(/*? shmem_symbol ?*/);
}


/*# We need to create a notification badge of their notificaion in our cspace #*/
/*- do allocate_cap(other_interface, is_reader=False) -*/
/*- set notification = pop('notification') -*/

static void /*? me.interface.name ?*/_notify(void) {
    seL4_Signal(/*? notification ?*/);
}

/*# We need to get the badge that they will signal us on #*/
/*- do allocate_cap(me, is_reader=True) -*/
/*- set notification = pop('notification') -*/
/*- set badge = pop('badge') -*/

static seL4_CPtr /*? me.interface.name ?*/_notification(void) {
    return /*? notification ?*/;
}

seL4_Word /*? me.interface.name ?*/_notification_badge(void) {
    return /*? badge ?*/;
}

/*- set interface_name =  me.interface.type.name -*/

/*- set queue_id = macros.virtqueue_get_client_id(composition, me, configuration) -*/
/*- if queue_id is none or not isinstance(queue_id, six.integer_types) -*/
  /*? raise(Exception('%s.%s_id must be set to a number' % (me.instance.name, me.interface.name))) ?*/
/*- endif -*/

//This is called by camkes runtime during init.
static void __attribute__((constructor)) register_connector(void) {
/*- if interface_name == "VirtQueueDrv" -*/
    camkes_virtqueue_channel_register(/*? queue_id ?*/, "/*? me.interface.name ?*/", /*? me.interface.name ?*/_get_size(), (volatile void *) &/*? shmem_symbol ?*/,  /*? me.interface.name ?*/_notify, /*? me.interface.name ?*/_notification(), /*? me.interface.name ?*/_notification_badge(), VIRTQUEUE_DRIVER);
/*- else -*/
    camkes_virtqueue_channel_register(/*? queue_id ?*/, "/*? me.interface.name ?*/", /*? me.interface.name ?*/_get_size(), (volatile void *) &/*? shmem_symbol ?*/,  /*? me.interface.name ?*/_notify, /*? me.interface.name ?*/_notification(), /*? me.interface.name ?*/_notification_badge(), VIRTQUEUE_DEVICE);
/*- endif -*/
}
