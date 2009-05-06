# RRRD

A Ruby interface for RRD.


## Example

    rrd = RRRD::RRD.new('data.rrd')

Fetch the `:average`, `:min`, `:max`, or `:last`.

    rrd.fetch(:average, :resolution => 10, :starts_at => Time.now, :ends_at => Time.now)

    rrd.last

    rrd.last_update
