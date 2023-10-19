#ifndef VTKFITSWRITER_H
#define VTKFITSWRITER_H

#include <vtkImageWriter.h>

#include <fitsio.h>

class vtkFitsWriter : public vtkImageWriter
{
public:
    static vtkFitsWriter* New();
    vtkTypeMacro(vtkFitsWriter, vtkImageWriter);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    virtual void Write() override;

    vtkGetMacro(FileName, char*);
    vtkSetMacro(FileName, char*);
protected:
    vtkFitsWriter();
    ~vtkFitsWriter() override;

    char* FileName;

    void writeFITSHeader(fitsfile* FITSFile, vtkImageData* imageData);
};

#endif // VTKFITSWRITER_H
