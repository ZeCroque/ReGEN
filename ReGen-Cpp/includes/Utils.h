#ifndef MATH_H
#define MATH_H

namespace Math
{
	//Doesn't return tuples with duplicated elements
	template<class T> std::vector<std::vector<T> > customCartesianProduct(const std::vector<T>& inFirstFactor, const std::vector<T>& inSecondFactor)
	{   
		auto result = std::vector<std::vector<T> >();
		result.reserve(inFirstFactor.size() * inSecondFactor.size());

		for (const auto& firstFactorElement : inFirstFactor)
	    {
	        for (const auto& secondFactorElement : inSecondFactor)
	        {
	        	if(firstFactorElement != secondFactorElement)
	        	{
	        		result.emplace_back(std::vector<T>{firstFactorElement, secondFactorElement});
				}
	        }
	    }
		return result;
	}

	//Fusion tuples such as (4, (5,6)) => (4, 5, 6) and doesn't return tuples with duplicated elements
	template<class T> std::vector<std::vector<T> > customCartesianProduct(std::vector<T> inFirstFactor, const std::vector<std::vector<T> >& inSecondFactor)
	{
		auto result = std::vector<std::vector<T> >();
		result.reserve(inSecondFactor.size());
		for(const auto& firstFactorElement : inFirstFactor)
		{
	        for(const auto& secondFactorTuple : inSecondFactor)
	        {
	        	bool singleton = true;
        		std::vector<T> outTuple;
        		outTuple.reserve(secondFactorTuple.size() + 1);
        		for(auto secondFactorElement : secondFactorTuple)
        		{
        			if(secondFactorElement == firstFactorElement)
        			{
        				singleton = false;
        				break;
        			}
        			outTuple.emplace_back(secondFactorElement);
        		}        	
	        	if(singleton)
	        	{
	        		outTuple.emplace_back(firstFactorElement);
					result.emplace_back(outTuple);
				}
			}
		}
		return result;
	}
}


#endif // MATH_H
