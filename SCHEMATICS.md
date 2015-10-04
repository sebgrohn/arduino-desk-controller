# Schematics for Desk Controllers

## Schematic A

```
         ┌───────── C1 ───────────┐
         │                        │
purple ──┴── R1 ─── D1 (LED) ──┬──┴──── gray
                       →       │
                               │
                        Z1 (zener diode) ↓
                               │
                   ┌───────────┘
green ─── SW4 ──┐  │
          (up)  │  │
                ├──┴── J9 (key) ─── blue
red ───── SW3 ──┘
         (down)
```

| Color  | Pin | Cor. TP color |
|--------|-----|---------------|
| gray   | 1   | orange/white  |
| -      | 2   | orange        |
| -      | 3   | green/white   |
| blue   | 4   | blue          |
| green  | 5   | blue/white    |
| -      | 6   | green         |
| purple | 7   | brown/white   |
| red    | 8   | brown         |


## Schematic B

```
yellow ─── SW6 ──┐
                 │
purple ─── SW5 ──┤
                 │
green ──── SW4 ──┤
           (up)  │
                 ├── red
orange ─── SW3 ──┤
          (down) │
                 │
gray ───── SW2 ──┤
                 │
blue ───── SW1 ──┘
```

| Color  | Pin |
|--------|-----|
| gray   | 1   |
| blue   | 2   |
| -      | 3   |
| -      | 4   |
| orange | 5   |
| yellow | 6   |
| purple | 7   |
| red    | 8   |
| green  | 9   |


## Box Templates

```
┌───┬───┐   ╓───╥───╖   ╒═══╤═══╕   ╔═══╦═══╗
│   │   │   ║   ║   ║   │   │   │   ║   ║   ║
├───┼───┤   ╟───╫───╢   ╞═══╪═══╡   ╠═══╬═══╣
│   │   │   ║   ║   ║   │   │   │   ║   ║   ║
└───┴───┘   ╙───╨───╜   ╘═══╧═══╛   ╚═══╩═══╝
```
