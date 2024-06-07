/*
 * ntfs-3g_embox_compat.h
 *
 *  Created on: 02 juillet 2013
 *      Author: fsulima
 */

#ifndef BMV2_EMBOX_COMPAT_H_
#define BMV2_EMBOX_COMPAT_H_

#include <string>
#include <sstream>
namespace std
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}


#endif /* BMV2_EMBOX_COMPAT_H_ */
