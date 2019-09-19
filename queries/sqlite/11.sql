SELECT PS_PARTKEY, SUM(PS_SUPPLYCOST * PS_AVAILQTY) AS VAL
FROM PARTSUPP,
     SUPPLIER,
     NATION
WHERE PS_SUPPKEY = S_SUPPKEY
  AND S_NATIONKEY = N_NATIONKEY
  AND N_NAME = 'GERMANY'
GROUP BY PS_PARTKEY
HAVING SUM(PS_SUPPLYCOST * PS_AVAILQTY) > (SELECT SUM(PS_SUPPLYCOST * PS_AVAILQTY) * 0.0001000000
                                           FROM PARTSUPP,
                                                SUPPLIER,
                                                NATION
                                           WHERE PS_SUPPKEY = S_SUPPKEY
                                             AND S_NATIONKEY = N_NATIONKEY
                                             AND N_NAME = 'GERMANY')
ORDER BY VAL DESC;
