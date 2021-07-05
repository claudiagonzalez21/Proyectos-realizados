
  SELECT EXTRACT(year from "rental_date") as "year", COUNT(*) film_id
  FROM rental
  GROUP BY year
  ORDER BY 1
