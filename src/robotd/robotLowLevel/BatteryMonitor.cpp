#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(Command* command)
{
	if (command == NULL) {
		printf("Invalid command\n");
		exit(EXIT_FAILURE);
	}
	EventBattery = NULL;
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
	} while (v < 63);
	printf("Battery filter convergence in %d mili seconds\n", i * interval_ms);
	filterAcc = 0;
}


double BatteryMonitor::ConvertVoltagePercent(double voltage, bool isCharging)
{
	const double minVoltage = 12.0;
	double maxVoltage = 15.5;
	double percent;

	if (isCharging) {
		maxVoltage = 16.8;
	}

	percent = voltage - minVoltage;
	percent = 100.0 * percent / (maxVoltage - minVoltage);

	return percent;
}

double BatteryMonitor::FilterVoltage(double voltage)
{
	double error;

	error = (double)voltage - filterAcc;

	filterAcc += error * (double)interval_ms / 1000000.0;

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
		else
		{
			printf("Battery FastProbe error [%d]\n", rc);
			sleep(5);
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
				percent = ConvertVoltagePercent(voltage, false);
				printf("Battery: [%.2fV] [%.4fV] [%.2f%%]\n", v, voltage, percent);

				if (EventBattery != NULL)
					EventBattery(percent);
			}
			else
			{
				printf("GetBattery CMD Error\n");
				sleep(5);
			}
		}
		else
		{
			printf("Battery monitor have no CMD\n");
			sleep(5);
		}
		usleep(interval_ms * 1000);
	}
}
