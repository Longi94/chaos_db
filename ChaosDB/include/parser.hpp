#pragma once

namespace chaos
{
    namespace parser
    {
        /**
         * procmaps_struct
         * @desc hold all the information about an area in the process's  VM
         */
        typedef struct procmaps_struct
        {
            void* addr_start; //< start address of the area
            void* addr_end; //< end address
            unsigned long length; //< size of the range

            char perm[5]; //< permissions rwxp
            short is_r; //< rewrote of perm with short flags
            short is_w;
            short is_x;
            short is_p;

            long offset; //< offset
            char dev[12]; //< dev major:minor
            int inode; //< inode of the file that backs the area

            char pathname[600]; //< the path of the file that backs the area
            //chained list
            struct procmaps_struct* next; //<handler of the chinaed list
        } procmaps_struct;
    }
}
