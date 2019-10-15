COPY INTO region   FROM '../tpch/tables/region.tbl'   USING DELIMITERS '|', '\n' LOCKED;
COPY INTO nation   FROM '../tpch/tables/nation.tbl'   USING DELIMITERS '|', '\n' LOCKED;
COPY INTO supplier FROM '../tpch/tables/supplier.tbl' USING DELIMITERS '|', '\n' LOCKED;
COPY INTO customer FROM '../tpch/tables/customer.tbl' USING DELIMITERS '|', '\n' LOCKED;
COPY INTO part     FROM '../tpch/tables/part.tbl'     USING DELIMITERS '|', '\n' LOCKED;
COPY INTO partsupp FROM '../tpch/tables/partsupp.tbl' USING DELIMITERS '|', '\n' LOCKED;
COPY INTO orders   FROM '../tpch/tables/orders.tbl'   USING DELIMITERS '|', '\n' LOCKED;
COPY INTO lineitem FROM '../tpch/tables/lineitem.tbl' USING DELIMITERS '|', '\n' LOCKED;
