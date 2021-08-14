#include <filesystem>

#include "CommandsDeclaration.h"
#include "DataManager.h"
#include "Scheduler.h"

int main()
{
	DataManager::getInstance()->init("one_story");
	declareCommands();

	Scheduler::init();
	const auto generationStartTimeStamp = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < DataManager::getInstance()->getTestLayout().numStoryToGenerate; ++i)
	{
		Scheduler scheduler("narrative" + std::to_string(i));
		scheduler.run();
	}
	const auto generationTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - generationStartTimeStamp).count();

	const std::string outputPath = "./Output";
	if(!std::filesystem::exists(outputPath))
	{
		std::filesystem::create_directories(outputPath);
	}
	assert(!std::filesystem::is_regular_file(outputPath));
	
	const auto fullPath = outputPath + "/" + "statistics.txt";
	if(std::ofstream file(fullPath, std::ios::out | std::ios::trunc); file)
	{
		file << "Generation Time: " << generationTimeNs << "ns (" << static_cast<double>(generationTimeNs) / 1000000000. << "s)" << std::endl;
		file << "Max physical RAM usage: " << static_cast<double>(Scheduler::maxPhysicalMem) / (1024. * 1024.) << "Mio"<< std::endl;
		file << "Min physical RAM usage: " << static_cast<double>(Scheduler::minPhysicalMem) / (1024. * 1024.) << "Mio"<< std::endl;
		file << "Average physical RAM usage:" << (static_cast<double>(Scheduler::physMemSum) / static_cast<double>(Scheduler::measureCount)) / (1024. * 1024.) << "Mio"<< std::endl;
		file << "Max virtual RAM usage:" << static_cast<double>(Scheduler::maxVirtualMem) / (1024. * 1024.) << "Mio" << std::endl;
		file << "Min virtual RAM usage:" << static_cast<double>(Scheduler::minVirtualMem) / (1024. * 1024.) << "Mio" << std::endl;
		file << "Average virtual RAM usage:" << (static_cast<double>(Scheduler::virtualMemSum) / static_cast<double>(Scheduler::measureCount)) / (1024. * 1024.) << "Mio" << std::endl;
		file << "Max CPU usage:" << Scheduler::maxCpuUsage << "%" << std::endl;
		file << "Min CPU usage:" << Scheduler::minCpuUsage << "%" << std::endl;
		file << "Average CPU usage:" << Scheduler::cpuUsageSum / static_cast<double>(Scheduler::measureCount) << "%" << std::endl;
	}


	return 0;
}
