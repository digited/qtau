#include <QtWidgets/QApplication>
#include "rocatool.h"
#include "Interface.h"

RocaTool *pRT = 0;


void LoadWav(char *path)
{
    pRT->onLoadWav(QString(path));
}

void UpdateSpectrum1(float *data, int dataLen, FECSOLAState params)
{
    pRT->setBeforeSpectrum(data, dataLen, params);
}

void UpdateSpectrum2(float *data, int dataLen, FECSOLAState params)
{
    pRT->setAfterSpectrum(data, dataLen, params);
}

void Synthesis(float* /*DestWave*/, FECSOLAState /*parameters*/)
{
    //
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    RocaTool rt;
    pRT = &rt;
    rt.show();

    return app.exec();
}

