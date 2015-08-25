# Algorithm for calculating desk up/down speed.

```
dh_actual_0 / dh_target * v_0 = v_1
dh_actual_1 / dh_target * v_1 = v_2
                              …
dh_actual_n / dh_target * v_n = v_(n+1)
```

where
* `dh`: height diff (_m_)
* `v`:  velocity (_m_/_s_)

## Example
```
(1,117 m - 0,7 m) / (1,11 m - 0,7 m) * 0,0195 m/s = 0,0198 m/s
(1,109 m - 0,7 m) / (1,11 m - 0,7 m) * 0,0198 m/s = …
```
