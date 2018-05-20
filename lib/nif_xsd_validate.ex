defmodule NifXsd do
    @on_load :load_nifs
  
    @doc false
    def load_nifs do
      :ok = :erlang.load_nif(Path.join(:code.priv_dir(:nif_xsd_validate), "nif_xsd_validate"), 0) 
    end

    @doc """
    Takes resource(xmlSchemaPtr) to validate the xml string.
    Use `NifXsd.Schema` to manage resources.
    ## Example
        NifXsd.validate(NifXsd.Schema.get(:someSchemaKey1), "<xml></xml>")
    """ 
    def validate(_resource, _xml) do
      raise "NIF validate/2 not implemented, load_nif probably failed"
    end

    @doc false
    def load_schema(_path) do
        raise "NIF load_schema/1 not implemented, load_nif probably failed"
      end  
  end