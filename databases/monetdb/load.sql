COPY INTO region   FROM '{DIR}/../tpch/tables/region.tbl'   USING DELIMITERS '|', '\n' LOCKED;
COPY INTO nation   FROM '{DIR}/../tpch/tables/nation.tbl'   USING DELIMITERS '|', '\n' LOCKED;
COPY INTO supplier FROM '{DIR}/../tpch/tables/supplier.tbl' USING DELIMITERS '|', '\n' LOCKED;
COPY INTO customer FROM '{DIR}/../tpch/tables/customer.tbl' USING DELIMITERS '|', '\n' LOCKED;
COPY INTO part     FROM '{DIR}/../tpch/tables/part.tbl'     USING DELIMITERS '|', '\n' LOCKED;
COPY INTO partsupp FROM '{DIR}/../tpch/tables/partsupp.tbl' USING DELIMITERS '|', '\n' LOCKED;
COPY INTO orders   FROM '{DIR}/../tpch/tables/orders.tbl'   USING DELIMITERS '|', '\n' LOCKED;
COPY INTO lineitem FROM '{DIR}/../tpch/tables/lineitem.tbl' USING DELIMITERS '|', '\n' LOCKED;
