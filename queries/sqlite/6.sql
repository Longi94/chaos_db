SELECT SUM(L_EXTENDEDPRICE * L_DISCOUNT) AS REVENUE
FROM LINEITEM
WHERE L_SHIPDATE >= '1994-01-01'
  AND L_SHIPDATE < '1995-01-01'
  AND L_DISCOUNT BETWEEN .06 - 0.01 AND .06 + 0.01
  AND L_QUANTITY < 24;
