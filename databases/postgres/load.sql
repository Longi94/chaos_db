COPY customer FROM '../tpch/tables/customer.tbl' WITH DELIMITER AS '|';
COPY lineitem FROM '../tpch/tables/lineitem.tbl' WITH DELIMITER AS '|';
COPY nation FROM '../tpch/tables/nation.tbl' WITH DELIMITER AS '|';
COPY orders FROM '../tpch/tables/orders.tbl' WITH DELIMITER AS '|';
COPY part FROM '../tpch/tables/part.tbl' WITH DELIMITER AS '|';
COPY partsupp FROM '../tpch/tables/partsupp.tbl' WITH DELIMITER AS '|';
COPY region FROM '../tpch/tables/region.tbl' WITH DELIMITER AS '|';
COPY supplier FROM '../tpch/tables/supplier.tbl' WITH DELIMITER AS '|';
