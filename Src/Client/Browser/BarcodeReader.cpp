#include "BarcodeReader.h"
#include "MyWebView.h"
#include "clientWindow.h"
#include "MyThread.h"


BarcodeReader::BarcodeReader(int decodeTimeoutLimit) : decodeTimeoutLimit(decodeTimeoutLimit)
{
    this->setObjectName("BarcodeDecoder");

    MyThread* myThread = new MyThread(this);

    this->moveToThread(myThread);

    myThread->start();
}
BarcodeReader::~BarcodeReader()
{
}


DmtxImage* BarcodeReader::convertImage(QImage image)
{
    // Taken from https://gitorious.org/livetransfer/3rdparty/trees/master/maemo-barcode


    // convert to an appropriate format
    image = image.convertToFormat(QImage::Format_RGB888);

    // we'll pad the image to a 4byte multiple of width
    // this is necessary for the libdmtx code which converts the buffer into a QImage to change its format
    // it also means we can then look at the returned buffer more easily too!

    int bpp = 24;
    int padding;

    if(image.bytesPerLine() == bpp/8*image.width())
    {
        padding = 0; // we're already a round number, so we're fine
    }
    else
    {            
        padding = 0;
        int remainingBytes = image.bytesPerLine()-bpp/8*image.width();
        while((padding*bpp/8 - remainingBytes) % 4)
        {
            padding++; // iterate until we've added enough padding pixels to be divisible by 4 (bytes)
        }
    }

    int width = image.width()+padding; // this is the padded width


    // the line data in the QImage are aligned on 32bit boundaries
    // but the decoder expects a contiguous chunk of data
    // so we need to generate that

    uchar* imageBuffer = (uchar*)malloc(width*image.height()*bpp/8*sizeof(char));

    if(!imageBuffer)
    {
        qDebug("Memory allocation error, convertImage aborted");
        return NULL; // memory allocation error
    }


    uchar* pos = imageBuffer;

    for(int i=0; i<image.height(); ++i)
    {
        memcpy(pos, image.constScanLine(i), image.width()*bpp/8); //image.bytesPerLine()

        pos = pos + image.width()*bpp/8;

        // add padding pixels
        for(int j=0; j<padding; ++j) // iterate through the padding pixels
        {   
            for(int k=0; k<bpp/8; ++k) // iterate though the components of the pixel
            { 
                *pos = 0; // give the pixel component a value
                pos++;    // move to the next one
            }
        }
    }


    DmtxImage* dmtxImage = dmtxImageCreate(imageBuffer, width, image.height(), DmtxPack24bppRGB);

    return dmtxImage;
}
QString BarcodeReader::decodeImage(DmtxImage* image)
{
    DmtxDecode *dmtxDecoder;
    DmtxRegion *dmtxRegion;
    DmtxMessage *dmtxMessage;
    DmtxTime dmtxTimeout;

    QByteArray message;


    dmtxDecoder = dmtxDecodeCreate(image, 1);

    dmtxTimeout = dmtxTimeAdd(dmtxTimeNow(), decodeTimeoutLimit);
    dmtxRegion  = dmtxRegionFindNext(dmtxDecoder, &dmtxTimeout);


    if(dmtxRegion != NULL) 
    {
        dmtxMessage = dmtxDecodeMatrixRegion(dmtxDecoder, dmtxRegion, DmtxUndefined);

        if(dmtxMessage != NULL) 
        {
            message = QByteArray((const char*)dmtxMessage->output, dmtxMessage->outputIdx);
            dmtxMessageDestroy(&dmtxMessage);
        }
        dmtxRegionDestroy(&dmtxRegion);
    }


    dmtxDecodeDestroy(&dmtxDecoder);
    free(image->pxl);
    dmtxImageDestroy(&image);


    return QString(message);
}

void BarcodeReader::readTimeStamp(qint64 screenShotTime, QImage screenShot1, QImage screenShot2) 
{
    screenShotMyLocalTime = screenShotTime;


    DmtxImage* dmtxScreenShot1 = convertImage(screenShot1);

    if(!dmtxScreenShot1)
    {
        qDebug() << "Invalid image data, screenShot1";
        return;
    }

    DmtxImage* dmtxScreenShot2 = convertImage(screenShot2);

    if(!dmtxScreenShot2)
    {
        qDebug() << "Invalid image data, screenShot2";
        return;
    }


    QString screenShot1String = decodeImage(dmtxScreenShot1);

    if(screenShot1String.isEmpty())
    {
        qDebug() << "No barcode detected (ScreenShot1)";
        return;
    }


    QString screenShot2String = decodeImage(dmtxScreenShot2);

    if(screenShot2String.isEmpty())
    {
        qDebug() << "No barcode detected (ScreenShot2)";
        return;
    }


    qint64 videotimestamp = screenShot2String.toLongLong();
    int offset = screenShotMyLocalTime - videotimestamp;


    if(screenShot1String == screenShot2String)
    {
        qDebug() << "Decoded the barcode successfully (" << offset << "ms delay) but the video appears to be frozen, aborting sync";
        return;
    }


    QStringList message;
    message << "videoOffset";
    message << QString::number(offset);

    emit this->ipcVideoOffset(message);

}

