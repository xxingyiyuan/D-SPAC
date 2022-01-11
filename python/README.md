## File Description

/data/dataset -----> Raw data selected for a specific time period (9:00 ~ 9:30)

generalRequest.py -----> used to generate requests

DQNPriority.py -----> DQN model

run_Rideshairng.py -----> algorithm DRL

## Build Setup

1. Configure parameters for different data sets in file [environment.py](./environment.py)

   ```python
   # Beijing dataset
   G = Generator(*settings.beijingRange)
   # Guangzhou dataset
   G = Generator(*settings.guangzhouRange) 
   # NewYork dataset
   G = Generator(*settings.newyorkRange)
   ```

2. Run Demo

`python run_Ridesharing.py`

