# Calculate Desk Speed

The following algorithm can be used for calculating the desk's up or down speed:

```
dh_actual_0 / dh_target * v_0 = v_1
dh_actual_1 / dh_target * v_1 = v_2
                              …
dh_actual_n / dh_target * v_n = v_(n+1)
```

where

* `dh_target`   is the configured target height difference (m),
* `dh_actual_i` is the actual measured height difference (m) for each iteration _i_, and
* `v_(i+1)`     is the computed velocity (m/s) for each iteration _i_.

Repeat _n_ times until sufficient precision is reached (`v_(i+1)` - `v_i` is sufficiently small).


## Example

```
(1,117 m - 0,7 m) / (1,11 m - 0,7 m) * 0,0195 m/s = 0,0198 m/s
(1,109 m - 0,7 m) / (1,11 m - 0,7 m) * 0,0198 m/s = …
```
