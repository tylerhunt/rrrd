require File.join(File.dirname(__FILE__), '..', 'spec_helper.rb')

describe "info" do
  before(:each) do
    @info = RRRD::RRD.new('spec/rrd/cpu-user.rrd').info
  end

  it "should provide access to the filename" do
    @info.filename.should == 'spec/rrd/cpu-user.rrd'
  end

  it "should provide access to the last update" do
    @info.last_update.should == Time.at(1226430198)
  end

  it "should provide access to the step" do
    @info.step.should == 10
  end

  it "should provide access to the version" do
    @info.version.should == '0003'
  end

  describe "data sources" do
    before(:each) do
      @data_sources = @info.data_sources
      @data_source = @data_sources.values.first
    end

    it "should have a single data source" do
      @data_sources.size.should == 1
    end

    it "should provide access to the type" do
      @data_source[:type].should == 'COUNTER'
    end

    it "should provide access to the maximum value" do
      @data_source[:max].should == 4294967295.0
    end

    it "should provide access to the minimum value" do
      @data_source[:min].should == 0.0
    end

    it "should provide access to the current value" do
      @data_source[:value].should == 0.8
    end

    it "should provide access to the unknown_sec" do
      @data_source[:unknown_sec].should == 0
    end

    it "should provide access to the last_ds" do
      @data_source[:last_ds].should == '200721'
    end

    it "should provide access to the minimal heartbeat" do
      @data_source[:minimal_heartbeat].should == 20
    end
  end

  describe "archives" do
    before(:each) do
      @archives = @info.archives
      @archive = @archives.values.first
    end

    it "should have twelve data sources" do
      @archives.size.should == 12
    end

    it "should provide access to the consolidation function" do
      @archive[:cf].should == 'MAX'
    end

    it "should provide access to the number of rows" do
      @archive[:rows].should == 1210
    end

    it "should provide access to the xfiles factor" do
      @archive[:xff].should == 0.1
    end

    describe "consolidated data point preparatory values" do
      it "should have one value" do
        @archive[:cdp_prep].should have(1).value
      end

      it "should provide access to the value" do
        (@archive[:cdp_prep].first[:value] * 100).round.should == 398
      end

      it "should provide access to the number of unknown data points" do
        @archive[:cdp_prep].first[:unknown_datapoints].should == 0
      end
    end

    it "should provide access to the consolidated data point preparatory values" do
      @archive[:cdp_prep].should have(1).values
    end

    it "should provide access to the number of primary data points per row" do
      @archive[:pdp_per_row].should == 50
    end
  end
end
