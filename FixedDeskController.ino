/*
template <class T1, class T2>
struct pair_value_less : std::binary_function<std::pair<T1, T2>, std::pair<T1, T2>, bool> {
  bool operator() (const std::pair<T1, T2>& x, const std::pair<T1, T2>& y) const { return x.second < y.second; }
};

class FixedHeightDeskController : public DeskController {  
public:
  std::pair<String, double> heights[];
  
  FixedHeightDeskController(const DeskControllerParams& params, const int& upPin, const int& downPin, const double& initialHeight, std::pair<String, double> heights [])
      : DeskController(params, upPin, downPin, initialHeight), heights(heights) {
    std::sort(heights, pair_value_less<String, double>());
    
    //targetHeightIt = this->heights->begin();
  }
  
private:
  std::pair<String, double> const * targetHeightIt;
};
*/
