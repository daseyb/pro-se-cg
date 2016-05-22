/***********************************************************************
 * Copyright 2011-2015 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/Utils/FileHelpers.hh>
#import <UIKit/UIKit.h>

namespace ACGL{
namespace Utils{
namespace FileHelpers{
            
std::string getDeviceDependentPathFor( const std::string &resource )
{
	NSString *res   = [NSString stringWithFormat:@"%s", resource.c_str() ];
	NSString *path = [[NSBundle mainBundle] pathForResource: res ofType: nil ];
    
    if (path) {
        return std::string( [path cStringUsingEncoding:1] );
    }
    return ""; // file not found
}

    std::string getDeviceResourcePath()
    {
        NSString *path = [[NSBundle mainBundle] resourcePath];
        
        return  std::string( [path cStringUsingEncoding:1] )+"/";
    }

}
}
}
