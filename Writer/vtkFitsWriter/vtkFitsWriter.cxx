#include "vtkFitsWriter.h"

#include <vtkDataArray.h>
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkType.h>

#include <cmath>
#include <unordered_map>

#include "fitsio.h"

std::unordered_map<int, int> vtkToFitsBitPixDataTypes{
    //                  VTKDataType,                FITSDataType
    {1, BYTE_IMG},      // VTK_BIT,                 FITS::BYTE_IMG (8)
    {2, BYTE_IMG},      // VTK_CHAR,                FITS::BYTE_IMG (8)
    {3, BYTE_IMG},      // VTK_UNSIGNED_CHAR,       FITS::BYTE_IMG (8)
    {4, SHORT_IMG},     // VTK_SHORT,               FITS::SHORT_IMG (16)
    {5, SHORT_IMG},     // VTK_UNSIGNED_SHORT,      FITS::SHORT_IMG (16)
    {6, LONG_IMG},      // VTK_INT,                 FITS::LONG_IMG (32)
    {7, LONG_IMG},      // VTK_UNSIGNED_INT,        FITS::LONG_IMG (32)
    {8, LONGLONG_IMG},  // VTK_LONG,                FITS::LONGLONG_IMG (64)
    {9, LONGLONG_IMG},  // VTK_UNSIGNED_LONG,       FITS::LONGLONG_IMG (64)
    {10, FLOAT_IMG},    // VTK_FLOAT,               FITS::FLOAT_IMG (-32)
    {11, DOUBLE_IMG},   // VTK_DOUBLE,              FITS::DOUBLE_IMG (-64)
    {16, LONGLONG_IMG}, // VTK_LONG_LONG,           FITS::LONGLONG_IMG (64)
    {17, LONGLONG_IMG}, // VTK_UNSIGNED_LONG_LONG,  FITS::LONGLONG_IMG (64)
    {18, LONGLONG_IMG}, // VTK___INT64,             FITS::LONGLONG_IMG (64)
    {19, LONGLONG_IMG}, // VTK_UNSIGNED___INT64,    FITS::LONGLONG_IMG (64)
};

std::unordered_map<int, int> vtkToFitsPixDataTypes{
    //                  VTKDataType,                FITSDataType
    {1, TBIT},          // VTK_BIT,                 FITS::TBIT (1)
    {2, TBYTE},         // VTK_CHAR,                FITS::TBYTE (11)
    {3, TBYTE},         // VTK_UNSIGNED_CHAR,       FITS::TSBYTE (12)
    {4, TSHORT},        // VTK_SHORT,               FITS::TSHORT (21)
    {5, TUSHORT},       // VTK_UNSIGNED_SHORT,      FITS::TUSHORT (20)
    {6, TINT},          // VTK_INT,                 FITS::TINT (31)
    {7, TUINT},         // VTK_UNSIGNED_INT,        FITS::TUINT (30)
    {8, TLONG},         // VTK_LONG,                FITS::TLONG (41)
    {9, TULONG},        // VTK_UNSIGNED_LONG,       FITS::TULONG (40)
    {10, TFLOAT},       // VTK_FLOAT,               FITS::TFLOAT (42)
    {11, TDOUBLE},      // VTK_DOUBLE,              FITS::TDOUBLE (82)
    {16, TLONGLONG},    // VTK_LONG_LONG,           FITS::TLONGLONG (81)
    {17, TULONGLONG},   // VTK_UNSIGNED_LONG_LONG,  FITS::TULONGLONG (80)
    {18, TLONGLONG},    // VTK___INT64,             FITS::TLONGLONG (81)
    {19, TULONGLONG},   // VTK_UNSIGNED___INT64,    FITS::TULONGLONG (80)
};

vtkStandardNewMacro(vtkFitsWriter);

vtkFitsWriter::vtkFitsWriter()
{
    this->FileName = nullptr;
}

vtkFitsWriter::~vtkFitsWriter()
{
//    if (this->FileName)
//        delete[] this->FileName;
}

void vtkFitsWriter::PrintSelf(ostream &os, vtkIndent indent)
{
    Superclass::PrintSelf(os, indent);
}

void vtkFitsWriter::writeFITSHeader(fitsfile *FITSFile, vtkImageData *imageData)
{
    int status = 0;

    std::string comment = "The software that processed this data";
    std::string val = "ParaviewFitsWriter";
    auto refVal = val.data();
    fits_write_key(FITSFile, TSTRING, "SOFTNAME", refVal, comment.c_str(), &status);
    if (status)
    {
        vtkErrorMacro("Error " + std::to_string(status) + " when writing SOFTNAME to FITS header in vtkFitsWriter::writeFITSHeader()!");
    }

    comment = "Version of the software";
    val = "0.0.1";
    refVal = val.data();
    fits_write_key(FITSFile, TSTRING, "SOFTVERS", refVal, comment.c_str(), &status);
    if (status)
    {
        vtkErrorMacro("Error " + std::to_string(status) + " when writing SOFTVERS to FITS header in vtkFitsWriter::writeFITSHeader()!");
    }

    comment = "Release date of the software";
    val = "2023-10-12";
    refVal = val.data();
    fits_write_key(FITSFile, TSTRING, "SOFTDATE", refVal, comment.c_str(), &status);
    if (status)
    {
        vtkErrorMacro("Error " + std::to_string(status) + " when writing SOFTDATE to FITS header in vtkFitsWriter::writeFITSHeader()!");
    }

    comment = "Maintainer of the software";
    val = "Cilliers Pretorius <pietersieliepcp@gmail.com>";
    refVal = val.data();
    fits_write_key(FITSFile, TSTRING, "SOFTAUTH", refVal, comment.c_str(), &status);
    if (status)
    {
        vtkErrorMacro("Error " + std::to_string(status) + " when writing SOFTAUTH to FITS header in vtkFitsWriter::writeFITSHeader()!");
    }

    comment = "Institute responsible for the software";
    val = "INAF-OACT https://www.oact.inaf.it/";
    refVal = val.data();
    fits_write_key(FITSFile, TSTRING, "SOFTINST", refVal, comment.c_str(), &status);
    if (status)
    {
        vtkErrorMacro("Error " + std::to_string(status) + " when writing SOFTINST to FITS header in vtkFitsWriter::writeFITSHeader()!");
    }
}

void vtkFitsWriter::Write()
{
    if (!this->GetInput())
    {
        vtkErrorMacro("Input not set for FitsWriter::Write()!");
        return;
    }

    vtkImageData* imgData = this->GetInput();
    if (!imgData)
    {
        vtkErrorMacro("Input is not vtkImageData for FitsWriter::Write()!");
        return;
    }

    // Check if the output file name is provided
    if (!this->FileName || !this->FileName[0])
    {
        vtkErrorMacro("Output file name not specified for FitsWriter::Write()!");
        return;
    }

    int* extent = imgData->GetExtent();
    int dataType = vtkToFitsBitPixDataTypes.at(imgData->GetScalarType());

    fitsfile* newFitsFile;
    int status = 0;
    fits_create_file(&newFitsFile, this->FileName, &status);
    if (status)
    {
        vtkErrorMacro("Error " + std::to_string(status) + " opening FITS file for FitsWriter::Write()!");
        return;
    }

    int naxis = imgData->GetDataDimension();
    long naxes[naxis];
    for (int i = 0; i < naxis; ++i)
        naxes[i] = imgData->GetDimensions()[i];

    fits_create_img(newFitsFile, dataType, naxis, naxes, &status);
    if (status)
    {
        vtkErrorMacro("Error " + std::to_string(status) + " creating FITS file for FitsWriter::Write()!");
        return;
    }

    writeFITSHeader(newFitsFile, imgData);

    //Write image data
    dataType = vtkToFitsPixDataTypes.at(imgData->GetScalarType());
    long* fpixel = new long[naxis];
    for (int i = 0; i < naxis; ++i)
        fpixel[i] = 1;
    long nelements = (extent[1] - extent[0] + 1) * (extent[3] - extent[2] + 1);

    auto imgPixs = imgData->GetPointData()->GetScalars()->GetVoidPointer(0);
//    auto PixData = static_cast<unsigned short *>(imgPixs);
//    for (int i = 0; i < nelements; ++i){
//        vtkErrorMacro("Pixel value from copied values at index " + std::to_string(i) + " is " + std::to_string(PixData[i]));
//    }

    fits_write_pix(newFitsFile, dataType, fpixel, nelements, imgPixs, &status);
    if (status)
    {
        vtkErrorMacro("Error " + std::to_string(status) + " when writing FITS file via CFITSIO, datatype = " + std::to_string(dataType) + "!");
    }

    fits_close_file(newFitsFile, &status);
    if (status)
    {
        vtkErrorMacro("Error " + std::to_string(status) + " when closing new FITS file for FitsWriter::Write()!");
    }
    delete[] fpixel;
}
