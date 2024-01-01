#ifndef ELCWSETTINGS_H
#define ELCWSETTINGS_H

#include <ELCSettings.h>

class ELCWSettings : public ELCSettings
{
public:
    static ELCWSettings& instance();

private:
    void createDefault(const QString& iniPath) override;
};

#endif // ELCWSETTINGS_H
