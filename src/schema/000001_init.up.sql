create table if not exists users (
    user_id bigint primary key not null,
    user_name text,
    count_watermelons int not null default 0,
    last_grow timestamptz
);
