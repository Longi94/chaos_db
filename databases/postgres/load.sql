\copy customer FROM '../tpch/tables/customer.tbl' WITH DELIMITER AS '|';
\copy lineitem FROM '../tpch/tables/lineitem.tbl' WITH DELIMITER AS '|';
\copy nation FROM '../tpch/tables/nation.tbl' WITH DELIMITER AS '|';
\copy orders FROM '../tpch/tables/orders.tbl' WITH DELIMITER AS '|';
\copy part FROM '../tpch/tables/part.tbl' WITH DELIMITER AS '|';
\copy partsupp FROM '../tpch/tables/partsupp.tbl' WITH DELIMITER AS '|';
\copy region FROM '../tpch/tables/region.tbl' WITH DELIMITER AS '|';
\copy supplier FROM '../tpch/tables/supplier.tbl' WITH DELIMITER AS '|';
