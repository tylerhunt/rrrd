require File.join(File.dirname(__FILE__), '..', 'spec_helper.rb')

describe "fetch" do
  before(:each) do
    @rrd = RRRD::RRD.new('spec/rrd/cpu-user.rrd')
  end

  describe "consolidation functions" do
    it "should fetch using the AVERAGE consolidation function" do
      @rrd.fetch(:average).should have(8641).data_points
    end

    it "should fetch using the MIN consolidation function" do
      @rrd.fetch(:min).should have(8641).data_points
    end

    it "should fetch using the MAX consolidation function" do
      @rrd.fetch(:max).should have(8641).data_points
    end
  end

  describe "should take a start time" do
  end
end
