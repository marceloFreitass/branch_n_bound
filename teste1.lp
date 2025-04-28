\ Model bruno_bruck
\ LP format - for model browsing. Use MPS format to capture full model detail.
\ Signature: 0x1c95757191f88be8
Maximize
  5 x0 + 10 x1 + 8 x2
Subject To
 R0: 3 x0 + 5 x1 + 2 x2 <= 6
 R1: 4 x0 + 4 x1 + 4 x2 <= 7
Bounds
 x0 = 1
 x1 <= 1
 x2 <= 1
End
