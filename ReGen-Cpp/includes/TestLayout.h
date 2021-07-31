#ifndef TESTLAYOUT_H
#define TESTLAYOUT_H

struct TestLayout
{
	int numStoryToGenerate;
	int maxNumberOfRewrites;
	std::list<std::pair<std::string, int> > metricsToOptimize;
	std::list<std::string> metricsToAnalyze;	
};

#endif // TESTLAYOUT_H
