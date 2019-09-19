SELECT L_SHIPMODE,
       SUM(CASE WHEN O_ORDERPRIORITY = '1-URGENT' OR O_ORDERPRIORITY = '2-HIGH' THEN 1 ELSE 0 END)    AS HIGH_LINE_COUNT,
       SUM(CASE WHEN O_ORDERPRIORITY <> '1-URGENT' AND O_ORDERPRIORITY <> '2-HIGH' THEN 1 ELSE 0 END) AS LOW_LINE_COUNT
FROM ORDERS,
     LINEITEM
WHERE O_ORDERKEY = L_ORDERKEY
  AND L_SHIPMODE IN ('MAIL', 'SHIP')
  AND L_COMMITDATE < L_RECEIPTDATE
  AND L_SHIPDATE < L_COMMITDATE
  AND L_RECEIPTDATE >= '1994-01-01'
  AND L_RECEIPTDATE < '1995-10-01'
GROUP BY L_SHIPMODE
ORDER BY L_SHIPMODE;
