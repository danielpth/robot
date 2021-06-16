#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(Command* command)
{
	if (command == NULL) {
		printf("Invalid command\n");
		exit(EXIT_FAILURE);
	}
	ProbeFilter();
	cmd = command;
	isThreadRunning = false;
	threadBatteryMonitor = new thread(&BatteryMonitor::funcBatteryMonitor, this);
}

BatteryMonitor::~BatteryMonitor()
{
	isThreadRunning = false;
	if (threadBatteryMonitor)
		threadBatteryMonitor->join();
	delete threadBatteryMonitor;
}

void BatteryMonitor::ProbeFilter()
{
	int i;
	double v;

	filterAcc = 0;
	i = 0;
	do {
		v = FilterVoltage(100);
		i++;
		//printf("filtro=%d v=[%.4f]\n", i, v);
	} while (v < 99.999);
	printf("Battery filter convergence in %d mili seconds\n", i * interval_ms);
	filterAcc = 0;
}


double BatteryMonitor::ConvertVoltagePercent(double voltage)
{
	const double maxVoltage = 16.8;
	const double minVoltage = 12.0;
	double percent;

	percent = voltage - minVoltage;
	percent = 100.0 * percent / (maxVoltage - minVoltage);

	return percent;
}

double BatteryMonitor::FilterVoltage(double voltage)
{
	double error;

	error = (double)voltage - filterAcc;

	filterAcc += error * (double)interval_ms / 25000.0;
	
	return filterAcc;
}

void BatteryMonitor::FastProbe()
{
	int rc, i;
	unsigned long ts;
	float voltage;

	filterAcc = 0;
	i = 0;
	while (i < 30) {
		rc = cmd->GetBattery(&ts, &voltage);
		if (rc == PROTOCOL_OK)
		{
			filterAcc += voltage;
			i++;
		}
		usleep(100000);
	}
	filterAcc /= i;
}

void BatteryMonitor::funcBatteryMonitor()
{
	int rc;
	unsigned long ts;
	float v;

	printf("%s\n", __FUNCTION__);

	FastProbe();

	isThreadRunning = true;

	while (isThreadRunning) {
		if (cmd) {
			rc = cmd->GetBattery(&ts, &v);
			if (rc == PROTOCOL_OK)
			{
				voltage = FilterVoltage(v);
				percent = ConvertVoltagePercent(voltage);
				printf("Battery: [%.2fV] [%.2fV] [%.2f%%]\n", v, voltage, percent);

				if (percent < 1.0) {
					printf("Battery very low!\n");
					//Speak("Bateria muito baixa!");
					sleep(3);
					//Speak("Desligando");
					cmd->System("sudo poweroff");
				}
				else if (percent < 10.0) {
					printf("Battery low\n");
					//Speak("Bateria baixa");
				}
			}
		}
		usleep(interval_ms * 1000);
	}
}
