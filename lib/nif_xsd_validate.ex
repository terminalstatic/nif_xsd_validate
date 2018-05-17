defmodule ValidateXsd do
    @on_load :load_nifs
  
    def load_nifs do
      #:erlang.load_nif(Path.join(:code.lib_dir(:nif_xsd_validate), "/priv/nif_xsd_validate"), 0) 
      :ok = :erlang.load_nif(:code.lib_dir(:nif_xsd_validate) ++ '/priv/nif_xsd_validate', 0)
    end
  
    def validate(_resource, _xml) do
      raise "NIF fast_compare/0 not implemented"
    end

    # ValidateXsd.load_schema("/home/ubuntu/source/go/src/com.mondial-travel/jetpacific/monXml/xmldef/mon/monV3.xsd")
    # {:ok, schema} = ValidateXsd.load_schema("http://10.1.2.140/monWebService/shema/MondialB2B_API.xsd")
    def load_schema(_path) do
        raise "NIF fast_compare/0 not implemented"
      end  
  end