#pragma once
#ifndef FRAGENUMS_H
#define FRAGENUMS_H

namespace FRAGTV 
{
    namespace Socket 
    { 
        enum Socket
        { 
            Signature      = 0xBAbe5101, 
            Uncompressed   = 0, 
            Compressed     = 1, 
            Version        = 100, 
            MinimumVersion = 100 
        };
    };


    enum MessageType 
    { 
        BrowserMessage   = 1000, 
        DemoMessage      = 2000, 
        MotdMessage      = 3000,
    };


    namespace Browser
    {
        enum 
        { 
            VideoPosition = 1100,
            VideoHide, 
            VideoNormal, 
            VideoMaximize,
            VideoUrl,
            ChatUrl
        }; 
    };


    namespace Demo
    {
        enum 
        { 
            New = 2010, 
            Append, 
            Finished 
        };
    };


    namespace Motd
    {
        enum
        {
            Html = 3100
        };
    }
};

#endif // FRAGENUMS_H
