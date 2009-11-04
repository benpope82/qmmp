
#include <QCursor>
#include <QString>
#include <QPixmap>
#include <QImage>
#include <QByteArray>
#include <QFile>
#include <QBitmap>
#include <QtDebug>
#include "cursorimage.h"

QCursor createCursor(QString path)
{
	if (path.isEmpty())
        return QCursor();
	
	// read file headers
	QFile curFile(path);
	curFile.open(QIODevice::ReadOnly);
	QDataStream curStream(&curFile);
	curStream.setByteOrder(QDataStream::LittleEndian);
	
	struct {
		quint16 zero;
		quint16 type;
		quint16 icons;
	} header2;
	curStream >> header2.zero >> header2.type >> header2.icons;

	struct {
		quint8 width;
		quint8 height;
		quint8 ncolours;
		quint8 zero;
		quint16 xhot;
		quint16 yhot;
		quint32 bytes;
		quint32 dibOffset;
	} directory2;
	curStream >> directory2.width >> directory2.height >> directory2.ncolours >> directory2.zero >> directory2.xhot >> directory2.yhot >> directory2.bytes >> directory2.dibOffset;
	
	curFile.seek(directory2.dibOffset);
	
	// prepare a .bmp for delegating decoding to qt
	struct {
		unsigned char magic[2];
		quint32 size;
		quint32 zero;
		quint32 rdataOffset;
	} bmpHeader;
	int bmpHeaderSize = (2+4+4+4);
	struct {
		quint32 hdrSize;
		quint32 width;
		quint32 height;
		quint16 planes;
		quint16 bpp;
		quint32 compression;
		quint32 dataSize;
		quint32 unused1;
		quint32 unused2;
		quint32 unused3;
		quint32 unused4;
	} dibHeader;
	int dibHeaderSize = (4+4+4+2+2+4+4+4+4+4+4);
	
	bmpHeader.magic[0] = 'B'; bmpHeader.magic[1] = 'M';
	bmpHeader.zero = 0;
	bmpHeader.size = bmpHeaderSize + directory2.bytes;
	bmpHeader.rdataOffset = bmpHeaderSize + dibHeaderSize + directory2.ncolours * 4;
	
	curStream >> dibHeader.hdrSize >> dibHeader.width >> dibHeader.height >> dibHeader.planes >> dibHeader.bpp >> dibHeader.compression >> dibHeader.dataSize >> dibHeader.unused1 >> dibHeader.unused2 >> dibHeader.unused3 >> dibHeader.unused4;
	dibHeader.height >>= 1;
	
	// the bmp bytes are in 'bmpData'
	QByteArray bmpData;
	QDataStream bmpStream(&bmpData, QIODevice::WriteOnly);
	bmpStream.setByteOrder(QDataStream::LittleEndian);
	bmpStream.writeRawData((char*) bmpHeader.magic, 2);
	bmpStream << bmpHeader.size << bmpHeader.zero << bmpHeader.rdataOffset;
	bmpStream << dibHeader.hdrSize << dibHeader.width << dibHeader.height << dibHeader.planes << dibHeader.bpp << dibHeader.compression << dibHeader.dataSize << dibHeader.unused1 << dibHeader.unused2 << dibHeader.unused3 << dibHeader.unused4;
	bmpData.append(curFile.read(directory2.bytes - dibHeaderSize));
	
	// decode the image into 'pix'
	int width = directory2.width;
	int height = directory2.height;
	QImage image;
	image.loadFromData(bmpData);
	//qDebug() << image.rect() << path;
	QPixmap pix = QPixmap::fromImage(image);
	
	// now we need the mask (transparency)
	QByteArray maskData = bmpData.right((width * height) / 8);
	QImage maskImage = QBitmap::fromData(QSize(width, height), (const uchar*) maskData.constData(), QImage::Format_Mono).toImage().mirrored(FALSE, TRUE);
	maskImage.invertPixels();
	pix.setMask(QBitmap::fromImage(maskImage));
	
	return QCursor(pix, directory2.xhot, directory2.yhot);
}
