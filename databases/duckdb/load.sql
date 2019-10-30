.print 'Importing region...'
COPY region FROM '../tpch/tables/region.tbl' CSV DELIMITER '|';
.print 'Importing nation...'
COPY nation FROM '../tpch/tables/nation.tbl' CSV DELIMITER '|';
.print 'Importing supplier...'
COPY supplier FROM '../tpch/tables/supplier.tbl' CSV DELIMITER '|';
.print 'Importing customer...'
COPY customer FROM '../tpch/tables/customer.tbl' CSV DELIMITER '|';
.print 'Importing part...'
COPY part FROM '../tpch/tables/part.tbl' CSV DELIMITER '|';
.print 'Importing partsupp...'
COPY partsupp FROM '../tpch/tables/partsupp.tbl' CSV DELIMITER '|';
.print 'Importing orders...'
COPY orders FROM '../tpch/tables/orders.tbl' CSV DELIMITER '|';
.print 'Importing lineitem...'
COPY lineitem FROM '../tpch/tables/lineitem.tbl' CSV DELIMITER '|';
