create table packets ( id SERIAL, received_dt timestamp default now(), provider varchar(128), packet json  );
